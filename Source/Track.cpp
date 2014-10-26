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
Track::Track(MixerAudioSource &tracksMixer, int trackIndex, bool stereo, int outputChannels, DurationChangedCallback callback)
	: audioThumbnailCache(1)
	, trackIndex(trackIndex)
	, stereo(stereo)
	, tracksMixer(tracksMixer)
	, thread ("track")
	, durationChangedCallback(callback)
{
	formatManager.registerBasicFormats();
	thread.startThread (3);

	remappingAudioSource = new ChannelRemappingAudioSource(&transportSource, false);
	remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
	// TODO: Probably do this for outputChannels times.
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

	openButton = new TextButton("open");
	addAndMakeVisible(openButton);
	openButton->addMouseListener(this, false);

	soloButton = new TextButton("solo");
	soloButton->setClickingTogglesState(true);
	addAndMakeVisible(soloButton);
	muteButton = new TextButton("mute");
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
		transportSource.setGain(muteButton->getToggleState() ? 0.0 : 1.0);
	}
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