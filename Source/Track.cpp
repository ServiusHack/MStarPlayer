/*
  ==============================================================================

	Track.cpp
	Created: 21 Jan 2014 1:00:48am
	Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"

#include <sstream>

Track::Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback, float gain, bool mute)
	: m_audioThumbnailCache(1)
	, m_trackIndex(trackIndex)
	, m_stereo(stereo)
	, m_tracksMixer(tracksMixer)
	, m_thread("track")
	, m_durationChangedCallback(callback)
	, m_soloMute(soloMute)
	, m_soloChangedCallback(soloChangedCallback)
	, m_longestDuration(0)
	, m_progress(0)
	, m_playerGain(gain)
	, m_trackGain(1.0f)
	, m_playerMute(mute)
{
	m_formatManager.registerBasicFormats();
	m_thread.startThread(3);

	m_transportSource.setGain(gain);

	m_remappingAudioSource = new ChannelRemappingAudioSource(&m_transportSource, false);
	m_remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
	m_remappingAudioSource->setOutputChannelMapping(0, 0);
	m_remappingAudioSource->setOutputChannelMapping(1, 1);

	m_tracksMixer.addInputSource(m_remappingAudioSource, false);

	m_idLabel = new Label();
	addAndMakeVisible(m_idLabel);
	updateIdText();

	m_descriptionLabel = new Label();
	addAndMakeVisible(m_descriptionLabel);
	m_descriptionLabel->setText(getName(), sendNotification);
	m_descriptionLabel->setJustificationType(Justification::topLeft);

	m_volumeSlider = new Slider("volume");
	addAndMakeVisible(m_volumeSlider);
	m_volumeSlider->setRange(0, 2, 0.1);
	m_volumeSlider->setValue(1.0);
	m_volumeSlider->setSliderStyle(Slider::LinearVertical);
	m_volumeSlider->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	m_volumeSlider->addListener(this);

	m_editButton = new ImageButton("edit");
	Image editImage = ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
	m_editButton->setImages(true, true, true,
		editImage, 0.7f, Colours::transparentBlack,
		editImage, 1.0f, Colours::transparentBlack,
		editImage, 1.0f, Colours::transparentBlack,
		0.0f);
	addAndMakeVisible(m_editButton);
	m_editButton->addListener(this);

	m_openButton = new ImageButton("open");
	Image openImage = ImageFileFormat::loadFrom(BinaryData::folderopen_png, BinaryData::folderopen_pngSize);
	m_openButton->setImages(true, true, true,
		openImage, 0.7f, Colours::transparentBlack,
		openImage, 1.0f, Colours::transparentBlack,
		openImage, 1.0f, Colours::transparentBlack,
		0.0f);
	addAndMakeVisible(m_openButton);
	m_openButton->addMouseListener(this, false);

	m_soloButton = new ImageButton("solo");
	Image soloImage = ImageFileFormat::loadFrom(BinaryData::audioheadphones_png, BinaryData::audioheadphones_pngSize);
	m_soloButton->setClickingTogglesState(true);
	m_soloButton->addListener(this);
	m_soloButton->setImages(true, true, true,
		soloImage, 0.7f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::red.withAlpha(0.5f),
		0.0f);
	addAndMakeVisible(m_soloButton);

	m_muteButton = new ImageButton("mute");
	Image muteedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemuted_png, BinaryData::audiovolumemuted_pngSize);
	Image unmutedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemedium_png, BinaryData::audiovolumemedium_pngSize);
	m_muteButton->setImages(true, true, true,
		unmutedImage, 0.7f, Colours::transparentBlack,
		unmutedImage, 1.0f, Colours::transparentBlack,
		muteedImage, 1.0f, Colours::transparentBlack,
		0.0f);
	m_muteButton->setClickingTogglesState(true);
	m_muteButton->addListener(this);
	addAndMakeVisible(m_muteButton);

	//formatManager.registerBasicFormats();

	m_audioThumbnail = new AudioThumbnail(1000, m_formatManager, m_audioThumbnailCache);
	m_audioThumbnail->addChangeListener(this);
}

Track::~Track()
{
	m_tracksMixer.removeInputSource(m_remappingAudioSource);

	delete m_audioThumbnail.release();
}

void Track::setName(String name)
{
	Component::setName(name);
	m_descriptionLabel->setText(name, sendNotification);
}

void Track::setPlayerGain(float gain)
{
	m_playerGain = gain;
	updateGain();
}

void Track::setTrackGain(float gain)
{
	m_trackGain = gain;
	m_volumeSlider->setValue(gain);
	updateGain();
}

void Track::setPlayerMute(bool mute)
{
	m_playerMute = mute;
	updateGain();
}

void Track::buttonClicked(Button *button)
{
	if (button == m_muteButton) {
		updateGain();
	}
	else if (button == m_soloButton) {
		m_soloChangedCallback();
	}
	else if (button == m_editButton) {
		TrackSettingsChangedCallback settingsCallback = [this](String name) {
			setName(name);
		};

		VolumeChangedCallback volumeChangedCallback = [this](float gain) {
			setTrackGain(gain);
		};
		m_editDialog = ScopedPointer<TrackEditDialogWindow>(new TrackEditDialogWindow(getName(), m_trackGain, settingsCallback, volumeChangedCallback));
	}
}

void Track::sliderValueChanged(Slider* /*slider*/)
{
	setTrackGain(m_volumeSlider->getValue());
}

void Track::updateGain()
{
	bool mute = m_playerMute || m_muteButton->getToggleState() || (m_soloMute && !m_soloButton->getToggleState());
	m_transportSource.setGain(mute ? 0.0f : (m_playerGain * m_trackGain));
}

bool Track::isSolo() const
{
	return m_soloButton->getToggleState();
}

void Track::updateIdText()
{
	std::stringstream stream;
	stream << m_trackIndex;
	stream << " ";
	stream << (m_stereo ? "St" : "Mo");
	m_idLabel->setText(stream.str(), sendNotification);
}

void Track::changeListenerCallback(ChangeBroadcaster *source)
{
	if (source == m_audioThumbnail)
		repaint();
}


void Track::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent != m_openButton)
		return;

	loadFile();
}

void Track::loadFile()
{
	FileChooser myChooser ("Please select the audio file you want to load ...",
			File::getSpecialLocation (File::userHomeDirectory),
			m_formatManager.getWildcardForAllFormats());
	if (!myChooser.browseForFileToOpen())
		return;


	m_audioFile = File(myChooser.getResult());

	AudioFormatReader* reader = m_formatManager.createReaderFor(m_audioFile);

	if (reader->numChannels > 2) {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
			"AudioPlayerJuce",
			"The selected file has more than two channels. This is not supported."
			);
		return;
	}

	if (!m_stereo && reader->numChannels != 1) {
		int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
			"AudioPlayerJuce",
			"You have selected a stereo file for a mono track.",
			"Convert to stereo track",
			"Abort",
			this,
			nullptr
			);
		switch (result) {
		case 1:
			m_stereo = true;
			updateIdText();
			break;
		default:
			return;
		}
	}

	if (m_stereo && reader->numChannels != 2) {
		int result = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
			"AudioPlayerJuce",
			"You have selected a mono file for a stereo track.",
			"Convert to mono track",
			"Abort",
			this,
			nullptr
			);
		switch (result) {
		case 1:
			m_stereo = false;
			updateIdText();
			break;
		default:
			return;
		}
	}

	loadFileIntoTransport();

}

void Track::loadFileIntoTransport()
{
	AudioFormatReader* reader = m_formatManager.createReaderFor(m_audioFile);

	m_audioThumbnail->setSource(new FileInputSource(m_audioFile));
	repaint();

	m_currentAudioFileSource = new AudioFormatReaderSource(reader, true);
	// ..and plug it into our transport source
	m_transportSource.setSource(m_currentAudioFileSource,
								32768, // tells it to buffer this many samples ahead
								&m_thread, // this is the background thread to use for reading-ahead
								reader->sampleRate);

	updateGain();

	m_duration = m_transportSource.getLengthInSeconds();
	m_durationChangedCallback();
}

void Track::paint (Graphics& g)
{
	if (m_trackIndex > 1)
		g.drawLine(0.0f, 0.0f, static_cast<float>(getWidth()), 0.0f);

	const static int componentWidth = 100 + 40 + 40 + 20;
	int drawWidth = getWidth() - componentWidth;
	if (m_longestDuration != 0)
		drawWidth = static_cast<int>(drawWidth * m_audioThumbnail->getTotalLength() / m_longestDuration);
	m_audioThumbnail->drawChannels(g, Rectangle<int>(componentWidth, 0, drawWidth, getHeight()), 0, m_audioThumbnail->getTotalLength(), 1.0f);

	g.setColour(Colour(255, 0, 0));
	drawWidth = getWidth() - componentWidth;
	float lineX = componentWidth + static_cast<float>(drawWidth * m_progress);
	g.drawLine(lineX, 0.0f, lineX, static_cast<float>(getHeight()));
}

void Track::timerCallback()
{
	double position = m_transportSource.getCurrentPosition();
	m_progress = position / m_longestDuration;

	if (m_positionCallback)
		m_positionCallback(position);

	repaint();

	if (!m_transportSource.isPlaying())
		stopTimer();
}

XmlElement* Track::saveToXml() const
{
	XmlElement* element = new XmlElement("Track");

	element->setAttribute("stereo", m_stereo ? "true" : "false");
	element->setAttribute("mute", m_muteButton->getToggleState() ? "true" : "false");
	element->setAttribute("solo", m_soloButton->getToggleState() ? "true" : "false");
	element->setAttribute("gain", m_trackGain);


	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);

	if (m_audioFile != File::nonexistent) {
		XmlElement* fileXml = new XmlElement("File");
		fileXml->addTextElement(m_audioFile.getFullPathName());
		element->addChildElement(fileXml);
	}

	return element;
}

void Track::restoreFromXml(const XmlElement& element)
{
	m_stereo = element.getStringAttribute("stereo", "false") == "true";
	m_muteButton->setToggleState(element.getStringAttribute("mute", "false") == "true", sendNotification);
	m_soloButton->setToggleState(element.getStringAttribute("solo", "false") == "true", sendNotification);
	setTrackGain(static_cast<float>(element.getDoubleAttribute("gain", 1.0)));

	XmlElement* nameXml = element.getChildByName("Name");
	if (nameXml != nullptr)
		setName(nameXml->getAllSubText().trim());

	XmlElement* fileXml = element.getChildByName("File");
	if (fileXml != nullptr) {
		m_audioFile = File(fileXml->getAllSubText().trim());
		loadFileIntoTransport();
	}
}

void Track::resized()
{
	m_idLabel->setBounds(0, 0, 100, 20);
	m_descriptionLabel->setBounds(0, 20, 100, getHeight() - 20);

	static const int buttonWidth = 40;

	m_volumeSlider->setBounds(100 + 3, 3, 20, getHeight() - 6);
	
	m_editButton->setBounds(100 + 20 + 3, 3, buttonWidth - 6, getHeight() / 2 - 6);
	m_openButton->setBounds(100 + 20 + 3, 3 + getHeight() / 2, buttonWidth - 6, getHeight() / 2 - 6);

	m_soloButton->setBounds(100 + 20 + 3 + buttonWidth, 3, buttonWidth - 6, getHeight() / 2 - 6);
	m_muteButton->setBounds(100 + 20 + 3 + buttonWidth, 3 + getHeight() / 2, buttonWidth - 6, getHeight() / 2 - 6);
}

void Track::play()
{
	startTimer(50);
	m_transportSource.setPosition(0);
	m_transportSource.start();
}

void Track::pause()
{
	m_transportSource.stop();
	stopTimer();
}

void Track::stop()
{
	m_transportSource.stop();
	m_transportSource.setPosition(0);
	stopTimer();
}

std::vector<int> Track::getMapping()
{
	int numChannels = m_stereo ? 2 : 1;
	std::vector<int> mapping(numChannels, -1);
	for (size_t channel = 0; channel < mapping.size(); ++channel)
		mapping[channel] = (m_remappingAudioSource->getRemappedOutputChannel(channel));

	return mapping;
}

void Track::setOutputChannels(int outputChannels)
{
	m_remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
}

void Track::setOutputChannelMapping(int source, int target)
{
	m_remappingAudioSource->setOutputChannelMapping(source, target);
}

int Track::getNumChannels()
{
	return m_currentAudioFileSource->getAudioFormatReader()->numChannels;
}

double Track::getDuration()
{
	return m_duration;
}

void Track::setLongestDuration(double duration)
{
	m_longestDuration = duration;
	repaint();
}

void Track::setSoloMute(bool mute)
{
	m_soloMute = mute;
	updateGain();
}

void Track::setPositionCallback(PositionCallback callback)
{
	this->m_positionCallback = callback;
}