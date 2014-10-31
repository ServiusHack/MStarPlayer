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
	, openImage(Drawable::createFromImageData(BinaryData::open_svg, BinaryData::open_svgSize))
	, soloImage(Drawable::createFromImageData(BinaryData::headphones_svg, BinaryData::headphones_svgSize))
	, muteImage(Drawable::createFromImageData(BinaryData::mute_svg, BinaryData::mute_svgSize))
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
	descriptionLabel->setText("Bezeichnung", sendNotification);
	descriptionLabel->setJustificationType(Justification::topLeft);

	editButton = new TextButton("edit");
	addAndMakeVisible(editButton);

	openButton = new DrawableButton("open", DrawableButton::ImageFitted);
	openButton->setImages(openImage);
	addAndMakeVisible(openButton);
	openButton->addMouseListener(this, false);

	soloButton = new DrawableButton("solo", DrawableButton::ImageFitted);
	soloButton->setClickingTogglesState(true);
	soloButton->addListener(this);
	soloButton->setImages(soloImage);
	addAndMakeVisible(soloButton);

	muteButton = new DrawableButton("mute", DrawableButton::ImageFitted);
	muteButton->setImages(muteImage);
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
}

void Track::buttonClicked(Button *button)
{
	if (button == muteButton) {
		setMuteState();
	}
	else if (button == soloButton) {
		soloChangedCallback();
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
	if (myChooser.browseForFileToOpen())
	{
		audioFile = File(myChooser.getResult());
		
		AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

		if (reader->numChannels > 2) {
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
				"AudioPlayerJuce",
				"The selected file has more than two channels. This is not supported."
				);
			return;
		}

		if (!stereo && reader->numChannels != 1) {
			int result = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon,
				"AudioPlayerJuce",
				"You have selected a stereo file for a mono track.",
				"Convert to stereo track",
				"Play file in mono",
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
			int result = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon,
				"AudioPlayerJuce",
				"You have selected a mono file for a stereo track.",
				"Convert to mono track",
				"Play file in stereo",
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
}

void Track::paint (Graphics& g)
{
	int drawWidth = getWidth();
	if (m_longestDuration != 0)
		drawWidth = static_cast<int>(drawWidth * audioThumbnail->getTotalLength() / m_longestDuration);
	audioThumbnail->drawChannels(g, Rectangle<int>(100 + 40 + 40,0,drawWidth, getHeight()), 0, audioThumbnail->getTotalLength(), 1.0f);
}

void Track::resized()
{
	idLabel->setBounds(0, 0, 100, 20);
	descriptionLabel->setBounds(0, 20, 100, getHeight() - 20);
	
	editButton->setBounds(100, 0, 40, getHeight() / 2);
	openButton->setBounds(100, getHeight() / 2, 40, getHeight() / 2);

	soloButton->setBounds(100 + 40, 0, 40, getHeight() / 2);
	muteButton->setBounds(100 + 40, getHeight() / 2, 40, getHeight() / 2);
}

void Track::play()
{
	transportSource.setPosition (0);
	transportSource.start();
}

void Track::pause()
{
	transportSource.stop();
}

void Track::stop()
{
	transportSource.stop();
	transportSource.setPosition (0);
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