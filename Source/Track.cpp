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

//==============================================================================
Track::Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback, bool soloMute, DurationChangedCallback soloChangedCallback)
	: audioThumbnailCache(1)
	, trackIndex(trackIndex)
	, stereo(stereo)
	, tracksMixer(tracksMixer)
	, thread ("track")
	, durationChangedCallback(callback)
	, m_soloMute(soloMute)
	, soloChangedCallback(soloChangedCallback)
	, m_longestDuration(0)
	, progress(0)
{
	formatManager.registerBasicFormats();
	thread.startThread (3);

	remappingAudioSource = new ChannelRemappingAudioSource(&transportSource, false);
	remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
	remappingAudioSource->setOutputChannelMapping(0, 0);
	remappingAudioSource->setOutputChannelMapping(1, 1);

	tracksMixer.addInputSource(remappingAudioSource, false);

	idLabel = new Label();
	addAndMakeVisible(idLabel);
	updateIdText();

	descriptionLabel = new Label();
	addAndMakeVisible(descriptionLabel);
	descriptionLabel->setText(getName(), sendNotification);
	descriptionLabel->setJustificationType(Justification::topLeft);

	editButton = new ImageButton("edit");
	Image editImage = ImageFileFormat::loadFrom(BinaryData::configure_png, BinaryData::configure_pngSize);
	editButton->setImages(true, true, true,
		editImage, 0.7f, Colours::transparentBlack,
		editImage, 1.0f, Colours::transparentBlack,
		editImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.0f);
	addAndMakeVisible(editButton);
	editButton->addListener(this);

	openButton = new ImageButton("open");
	Image openImage = ImageFileFormat::loadFrom(BinaryData::folderopen_png, BinaryData::folderopen_pngSize);
	openButton->setImages(true, true, true,
		openImage, 0.7f, Colours::transparentBlack,
		openImage, 1.0f, Colours::transparentBlack,
		openImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.0f);
	addAndMakeVisible(openButton);
	openButton->addMouseListener(this, false);

	soloButton = new ImageButton("solo");
	Image soloImage = ImageFileFormat::loadFrom(BinaryData::audioheadphones_png, BinaryData::audioheadphones_pngSize);
	soloButton->setClickingTogglesState(true);
	soloButton->addListener(this);
	soloButton->setImages(true, true, true,
		soloImage, 0.7f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::transparentBlack,
		soloImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.0f);
	addAndMakeVisible(soloButton);

	muteButton = new ImageButton("mute");
	Image muteedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemuted_png, BinaryData::audiovolumemuted_pngSize);
	Image unmutedImage = ImageFileFormat::loadFrom(BinaryData::audiovolumemedium_png, BinaryData::audiovolumemedium_pngSize);
	muteButton->setImages(true, true, true,
		unmutedImage, 0.7f, Colours::transparentBlack,
		unmutedImage, 1.0f, Colours::transparentBlack,
		muteedImage, 1.0f, Colours::transparentBlack,
		0.0f);
	muteButton->setClickingTogglesState(true);
	muteButton->addListener(this);
	addAndMakeVisible(muteButton);

	//formatManager.registerBasicFormats();

	audioThumbnail = new AudioThumbnail(1000, formatManager, audioThumbnailCache);
	audioThumbnail->addChangeListener(this);
}

Track::~Track()
{
	tracksMixer.removeInputSource(remappingAudioSource);

	delete audioThumbnail.release();
}

void Track::setName(String name)
{
	Component::setName(name);
	descriptionLabel->setText(name, sendNotification);
}

void Track::buttonClicked(Button *button)
{
	if (button == muteButton) {
		setMuteState();
	}
	else if (button == soloButton) {
		soloChangedCallback();
	}
	else if (button == editButton) {
		TrackSettingsChangedCallback settingsCallback = [&](String name) {
			setName(name);
		};
		editDialog = ScopedPointer<TrackEditDialogWindow>(new TrackEditDialogWindow(getName(), settingsCallback));
	}
}

void Track::setMuteState()
{
	bool mute = muteButton->getToggleState() || (m_soloMute && !soloButton->getToggleState());
	transportSource.setGain(mute ? 0.0f : 1.0f);
}

bool Track::isSolo() const
{
	return soloButton->getToggleState();
}

void Track::updateIdText()
{
	std::stringstream stream;
	stream << trackIndex;
	stream << " ";
	stream << (stereo ? "St" : "Mo");
	idLabel->setText(stream.str(), sendNotification);
}

void Track::changeListenerCallback(ChangeBroadcaster *source)
{
	if (source == audioThumbnail)
		repaint();
}


void Track::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent != openButton)
		return;

	loadFile();
}

void Track::loadFile()
{
	FileChooser myChooser ("Please select the audio file you want to load ...",
			File::getSpecialLocation (File::userHomeDirectory),
			formatManager.getWildcardForAllFormats());
	if (!myChooser.browseForFileToOpen())
		return;


	audioFile = File(myChooser.getResult());

	AudioFormatReader* reader = formatManager.createReaderFor(audioFile);

	if (reader->numChannels > 2) {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
			"AudioPlayerJuce",
			"The selected file has more than two channels. This is not supported."
			);
		return;
	}

	if (!stereo && reader->numChannels != 1) {
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
			stereo = true;
			updateIdText();
			break;
		default:
			return;
		}
	}

	if (stereo && reader->numChannels != 2) {
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
			stereo = false;
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
	AudioFormatReader* reader = formatManager.createReaderFor(audioFile);

	audioThumbnail->setSource(new FileInputSource(audioFile));
	repaint();

	currentAudioFileSource = new AudioFormatReaderSource (reader, true);
	// ..and plug it into our transport source
	transportSource.setSource (currentAudioFileSource,
								32768, // tells it to buffer this many samples ahead
								&thread, // this is the background thread to use for reading-ahead
								reader->sampleRate);

	setMuteState();

	m_duration = transportSource.getLengthInSeconds();
	durationChangedCallback();
}

void Track::paint (Graphics& g)
{
	const static int componentWidth = 100 + 40 + 40;
	int drawWidth = getWidth() - componentWidth;
	if (m_longestDuration != 0)
		drawWidth = static_cast<int>(drawWidth * audioThumbnail->getTotalLength() / m_longestDuration);
	audioThumbnail->drawChannels(g, Rectangle<int>(componentWidth, 0, drawWidth, getHeight()), 0, audioThumbnail->getTotalLength(), 1.0f);

	g.setColour(Colour(255, 0, 0));
	drawWidth = getWidth() - componentWidth;
	float lineX = componentWidth + static_cast<float>(drawWidth * progress);
	g.drawLine(lineX, 0.0f, lineX, static_cast<float>(getHeight()));
}

void Track::timerCallback()
{
	double position = transportSource.getCurrentPosition();
	progress = position / m_longestDuration;

	if (positionCallback)
		positionCallback(position);

	repaint();

	if (!transportSource.isPlaying())
		stopTimer();
}

XmlElement* Track::saveToXml() const
{
	XmlElement* element = new XmlElement("Track");

	element->setAttribute("stereo", stereo ? "true" : "false");
	element->setAttribute("mute", muteButton->getToggleState() ? "true" : "false");
	element->setAttribute("solo", soloButton->getToggleState() ? "true" : "false");


	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);

	if (audioFile != File::nonexistent)
	{
		XmlElement* fileXml = new XmlElement("File");
		fileXml->addTextElement(audioFile.getFullPathName());
		element->addChildElement(fileXml);
	}

	return element;
}

void Track::restoreFromXml(const XmlElement& element)
{
	stereo = element.getStringAttribute("stereo", "false") == "true";
	muteButton->setToggleState(element.getStringAttribute("mute", "false") == "true", sendNotification);
	soloButton->setToggleState(element.getStringAttribute("solo", "false") == "true", sendNotification);

	XmlElement* nameXml = element.getChildByName("Name");
	if (nameXml != nullptr)
	{
		setName(nameXml->getAllSubText().trim());
	}

	XmlElement* fileXml = element.getChildByName("File");
	if (fileXml != nullptr)
	{
		audioFile = File(fileXml->getAllSubText().trim());
		loadFileIntoTransport();
	}
}

void Track::resized()
{
	idLabel->setBounds(0, 0, 100, 20);
	descriptionLabel->setBounds(0, 20, 100, getHeight() - 20);

	static const int buttonWidth = 40;
	
	editButton->setBounds(100 + 3, 3                  , buttonWidth - 6, getHeight() / 2 - 6);
	openButton->setBounds(100 + 3, 3 + getHeight() / 2, buttonWidth - 6, getHeight() / 2 - 6);

	soloButton->setBounds(100 + 3 + buttonWidth, 3                  , buttonWidth - 6, getHeight() / 2 - 6);
	muteButton->setBounds(100 + 3 + buttonWidth, 3 + getHeight() / 2, buttonWidth - 6, getHeight() / 2 - 6);
}

void Track::play()
{
	startTimer(50);
	transportSource.setPosition (0);
	transportSource.start();
}

void Track::pause()
{
	transportSource.stop();
	stopTimer();
}

void Track::stop()
{
	transportSource.stop();
	transportSource.setPosition (0);
	stopTimer();
}

std::vector<int> Track::getMapping()
{
	std::vector<int> mapping(currentAudioFileSource->getAudioFormatReader()->numChannels, -1);
	for (size_t channel = 0; channel < mapping.size(); ++channel) {
		mapping[channel] = (remappingAudioSource->getRemappedOutputChannel(channel));
	}
	return mapping;
}

void Track::setOutputChannels(int outputChannels)
{
	remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
}

void Track::setOutputChannelMapping(int source, int target)
{
	remappingAudioSource->setOutputChannelMapping(source, target);
}

int Track::getNumChannels()
{
	return currentAudioFileSource->getAudioFormatReader()->numChannels;
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
	setMuteState();
}

void Track::setPositionCallback(PositionCallback callback)
{
	this->positionCallback = callback;
}