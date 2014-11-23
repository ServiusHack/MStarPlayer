#include <list>
#include <stdint.h>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "JinglePlayerWindow.h"

namespace {
	const int ProgressBarHeight = 26;
	const int TotalDurationTextWidth = 70;
}

JinglePlayerWindow::JinglePlayerWindow(MixerComponent* mixer, OutputChannelNames *outputChannelNames, float gain, bool solo, bool mute)
	: m_mixer(mixer)
	, m_outputChannelNames(outputChannelNames)
	, m_thread("audio file preview")
	, m_playImage(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize))
	, m_stopImage(Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize))
	, m_progress(0.0)
	, m_gain(1.0f)
	, m_solo(solo)
	, m_soloMute(false) // might be updated later
	, m_mute(mute)
	, m_color(0xffffffff)
	, m_showRemainingTime(false)
	, m_blink(false)
	, m_paintColor(m_color)
	, m_outputChannels(outputChannelNames->getNumberOfChannels())
{
	// progress bar
	m_progressBar = new ProgressBar(m_progress);
	m_progressBar->setPercentageDisplay(false);
	m_progressBar->setTextToDisplay("00:00:000");
	m_progressBar->addMouseListener(this, false);
	addAndMakeVisible(m_progressBar);

	// total duration text
	m_totalDurationText = new Label();
	addAndMakeVisible(m_totalDurationText);
	m_totalDurationText->setJustificationType(Justification::centredRight);
	m_totalDurationText->setText("00:00:000", sendNotification);

	// play button
	m_playButton = new DrawableButton("Play", DrawableButton::ImageFitted);
	addAndMakeVisible(m_playButton);
	m_playButton->setImages(m_playImage);
	m_playButton->setEnabled(false);
	m_playButton->addListener(this);
	m_playButton->setEdgeIndent(30);

	// configuration button
	m_configureButton = new ImageButton("Configure");
	Image normalImage = ImageFileFormat::loadFrom (BinaryData::configure_png, BinaryData::configure_pngSize);
	m_configureButton->setImages(true, true, true,
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(m_configureButton);
	m_configureButton->addMouseListener(this, false);

	// filename label
	m_fileNameLabel = new Label("filename label");
	m_fileNameLabel->setColour(Label::textColourId, m_paintColor.contrasting(1.0f));
	m_fileNameLabel->setJustificationType(Justification::centred);
	addAndMakeVisible(m_fileNameLabel);
	
	// audio setup
	m_formatManager.registerBasicFormats();
	m_thread.startThread(3);

	m_remappingAudioSource = new ChannelRemappingAudioSourceWithVolume(&m_transportSource, false);
	m_remappingAudioSource->setNumberOfChannelsToProduce(outputChannelNames->getNumberOfChannels());
	// TODO: Probably do this for outputChannels times.
	m_remappingAudioSource->setOutputChannelMapping(0, 0);
	m_remappingAudioSource->setOutputChannelMapping(1, 1);
	mixer->getMixerAudioSource().addInputSource(m_remappingAudioSource, false);
	
	setGain(gain);
	mixer->registerPlayer(this);

	setBounds(0, 0, 300, 150);
}

JinglePlayerWindow::~JinglePlayerWindow()
{
	m_mixer->getMixerAudioSource().removeInputSource(m_remappingAudioSource);
	m_mixer->unregisterPlayer(this);
	m_transportSource.setSource(nullptr);
}

void JinglePlayerWindow::setGain(float gain)
{
	m_gain = gain;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->gainChanged(gain);
}

float JinglePlayerWindow::getGain() const
{
	return m_gain;
}

void JinglePlayerWindow::setPan(float pan)
{

	for (MixerControlableChangeListener *listener : m_listeners)
		listener->panChanged(pan);
}

float JinglePlayerWindow::getPan() const
{
	return 1.0f;
}

void JinglePlayerWindow::setSoloMute(bool soloMute)
{
	m_soloMute = soloMute;
	updateGain();
}

bool JinglePlayerWindow::getSoloMute() const
{
	return m_soloMute;
}

void JinglePlayerWindow::setSolo(bool solo)
{
	m_solo = solo;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->soloChanged(solo);
}

bool JinglePlayerWindow::getSolo() const
{
	return m_solo;
}

void JinglePlayerWindow::setMute(bool mute)
{
	m_mute = mute;
	updateGain();
	for (MixerControlableChangeListener *listener : m_listeners)
		listener->muteChanged(mute);
}

bool JinglePlayerWindow::getMute() const
{
	return m_mute;
}

float JinglePlayerWindow::getVolume() const
{
	float maxVolume = 0;
	for (int i = 0; i < m_outputChannels; ++i) {
		maxVolume = std::max(maxVolume, m_remappingAudioSource->getVolume(i));
	}

	return maxVolume;
}

void JinglePlayerWindow::updateGain()
{
	bool mute = m_mute || (m_soloMute && !m_solo);
	m_transportSource.setGain(mute ? 0.0f : m_gain);
}

void JinglePlayerWindow::setOutputChannels(int outputChannels)
{
	m_outputChannels = outputChannels;
	m_remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
}

std::vector<MixerControlable*> JinglePlayerWindow::getSubMixerControlables()
{
	return std::vector<MixerControlable*>();
}

void JinglePlayerWindow::resized()
{
	m_configureButton->setBounds(0, getHeight() - ProgressBarHeight, ProgressBarHeight, ProgressBarHeight);
	m_progressBar->setBounds(m_configureButton->getWidth(), getHeight() - ProgressBarHeight, getWidth() - m_configureButton->getWidth() - TotalDurationTextWidth, ProgressBarHeight);
	m_totalDurationText->setBounds(getWidth() - TotalDurationTextWidth, getHeight() - ProgressBarHeight, TotalDurationTextWidth, ProgressBarHeight);
	m_playButton->setBounds(0, 0, getWidth(), getHeight() - ProgressBarHeight);
	m_fileNameLabel->setBounds(0, getHeight() - ProgressBarHeight - ProgressBarHeight, getWidth(), ProgressBarHeight);
}

void JinglePlayerWindow::paint(Graphics& g)
{
	g.fillAll(m_paintColor);
}

void JinglePlayerWindow::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent == m_progressBar)
	{
		m_showRemainingTime = !m_showRemainingTime;
		timerCallback();
	}
	if (event.eventComponent == m_configureButton)
	{
		PopupMenu m;
		m.addItem (1, "load file");
		m.addItem(2, "configure channels", m_currentAudioFileSource != nullptr);
		m.addItem(3, "rename");
		const int result = m.show();

		switch (result) {
		case 1:
			loadFile();
			break;

		case 2:
			configureChannels();
			break;

		case 3:
			rename();
			break;
		}
	}
}

void JinglePlayerWindow::loadFile()
{
	FileChooser myChooser ("Please select the audio file you want to load ...",
			File::getSpecialLocation (File::userHomeDirectory),
			m_formatManager.getWildcardForAllFormats());
	
	if (!myChooser.browseForFileToOpen())
		return;

	m_audioFile = File(myChooser.getResult());
	loadFileIntoTransport();
}

void JinglePlayerWindow::configureChannels()
{
	if (m_channelMappingWindow.get() == nullptr) {
		m_channelMappingWindow.set(new ChannelMappingWindow(m_outputChannelNames, createMapping(), [&](int source, int target) {
			m_remappingAudioSource->setOutputChannelMapping(source, target);
		}, [&]() {
			// clear is not working
			delete m_channelMappingWindow.release();
		}), true);
	}
	m_channelMappingWindow->addToDesktop();
	m_channelMappingWindow->toFront(true);
}

void JinglePlayerWindow::rename()
{
	if (m_renameDialog.get() == nullptr) {
		m_renameDialog.set(new RenameDialogWindow(getName(), m_color, m_userImagePath, [this](String name) {
			setName(name);
		}, [this](Colour color) {
			m_color = color;
			m_mixer->updatePlayerColor(this, m_color);
			updatePointColor();
			repaint();
		}, [&]() {
			// clear is not working
			delete m_renameDialog.release();
		}, [this](juce::File file) {
			if (file == File::nonexistent)
			{
				m_userImagePath = "";
				delete m_userImage.release();
				m_playButton->setImages(m_transportSource.isPlaying() ? m_stopImage : m_playImage);
			}
			else
			{
				m_userImagePath = file.getFullPathName();
				m_userImage.set(Drawable::createFromImageFile(file), true);
				m_playButton->setImages(m_userImage.get());
			}
		}), true);
	}
	m_renameDialog->addToDesktop();
	m_renameDialog->toFront(true);
}

void JinglePlayerWindow::buttonClicked(Button * /*button*/)
{
	if (m_transportSource.isPlaying()) {
		m_playButton->setImages(m_userImage ? m_userImage.get() : m_playImage);
		m_transportSource.stop();
		m_transportSource.setPosition(0);
		stopTimer();
	}
	else {
		m_playButton->setImages(m_userImage ? m_userImage.get() : m_stopImage);
		m_transportSource.setPosition(0);
		m_transportSource.start();
		startTimer(50);
	}
}

void JinglePlayerWindow::loadFileIntoTransport ()
{
	stopTimer();

	// unload the previous file source and delete it..
	m_transportSource.setSource(nullptr);
	m_currentAudioFileSource = nullptr;
	m_playButton->setImages(m_userImage ? m_userImage.get() : m_playImage);

	m_fileNameLabel->setText(m_audioFile.getFileName(), sendNotification);

	AudioFormatReader* reader = m_formatManager.createReaderFor(m_audioFile);

	m_playButton->setEnabled(reader != nullptr);

	if (reader != nullptr)
	{
		m_currentAudioFileSource = new AudioFormatReaderSource(reader, true);

		// ..and plug it into our transport source
		m_transportSource.setSource(m_currentAudioFileSource,
								   32768, // tells it to buffer this many samples ahead
								   &m_thread, // this is the background thread to use for reading-ahead
								   reader->sampleRate);

		if (m_channelMappingWindow.get() != nullptr)
			m_channelMappingWindow->setMapping(createMapping());

		timerCallback();
	}
}

const std::vector<std::pair<char,int>> JinglePlayerWindow::createMapping()
{
	std::vector<std::pair<char, int>> mapping(m_currentAudioFileSource->getAudioFormatReader()->numChannels, std::pair<char, int>(0,-1));

	for (size_t channel = 0; channel < mapping.size(); ++channel)
	{
		char side = 'm';
		if (mapping.size() == 2)
			side = channel ? 'r' : 'l';
		mapping[channel] = std::pair<char, int>(side, (m_remappingAudioSource->getRemappedOutputChannel(channel)));
	}

	return mapping;
}

void JinglePlayerWindow::timerCallback()
{
	if (!m_currentAudioFileSource)
		return;

	double position    = m_transportSource.getCurrentPosition();
	double totalLength = m_transportSource.getLengthInSeconds();
	
	if (position > totalLength) {
		m_playButton->setImages(m_userImage ? m_userImage.get() : m_playImage);
		position = 0;
		m_progress = 0.0;
	} else {
		m_progress = position / totalLength;
	}

	double remainingTime = totalLength - position;
	
	m_progressBar->setTextToDisplay(Utils::formatSeconds(m_showRemainingTime ? remainingTime : position));
	m_totalDurationText->setText(Utils::formatSeconds(totalLength), sendNotification);

	if (remainingTime < 10) {
		double decimal = remainingTime - static_cast<long>(remainingTime);
		bool blink = decimal >= 0.5;
		if (blink != m_blink) {
			m_blink = blink;
			updatePointColor();
			repaint();
		}
	}
}

void JinglePlayerWindow::updatePointColor()
{
	if (m_blink)
		m_paintColor = m_color.contrasting(0.5f);
	else
		m_paintColor = m_color;

	m_fileNameLabel->setColour(Label::textColourId, m_paintColor.contrasting(1.0f));
}

XmlElement* JinglePlayerWindow::saveToXml() const
{
    XmlElement* element = new XmlElement("Player");
    element->setAttribute("type", "jingle");
	element->setAttribute("gain", m_transportSource.getGain());
	element->setAttribute("mute", m_mute);
	element->setAttribute("solo", m_solo);
	element->setAttribute("color", m_color.toString());
	if (m_userImage)
		element->setAttribute("image", m_userImagePath);
	element->setAttribute("showRemainingTime", m_showRemainingTime);

	XmlElement* boundsXml = new XmlElement("Bounds");
	Rectangle<int> bounds = getParentComponent()->getBounds();
	boundsXml->setAttribute("x", bounds.getX());
	boundsXml->setAttribute("y", bounds.getY());
	boundsXml->setAttribute("width", bounds.getWidth());
	boundsXml->setAttribute("height", bounds.getHeight());
	element->addChildElement(boundsXml);

	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);
	
	if (m_audioFile != File::nonexistent) {
		XmlElement* fileXml = new XmlElement("File");
		fileXml->addTextElement(m_audioFile.getFullPathName());
		element->addChildElement(fileXml);
	}

	XmlElement* channelMappingXml = new XmlElement("ChannelMapping");
	channelMappingXml->addChildElement(m_remappingAudioSource->createXml());
	element->addChildElement(channelMappingXml);

	return element;
}

void JinglePlayerWindow::restoreFromXml (const XmlElement& element)
{
	m_color = Colour::fromString(element.getStringAttribute("color", "0xffffffff"));
	if (element.hasAttribute("image")) {
		m_userImagePath = element.getStringAttribute("image");
		m_userImage.set(Drawable::createFromImageFile(File(m_userImagePath)), true);
		m_playButton->setImages(m_userImage.get());
	}

	m_showRemainingTime = element.getBoolAttribute("showRemainingTime", false);

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

	XmlElement* fileXml = element.getChildByName("File");
	if (fileXml != nullptr)
	{
		m_audioFile = File(fileXml->getAllSubText().trim());
		loadFileIntoTransport();
	}

	XmlElement* channelMappingXml = element.getChildByName("ChannelMapping");
	m_remappingAudioSource->restoreFromXml(*channelMappingXml->getChildElement(0));
}

void JinglePlayerWindow::SetChannelCountChangedCallback(ChannelCountChangedCallback callback)
{

}