/*
  ==============================================================================

	Track.cpp
	Created: 21 Jan 2014 1:00:48am
	Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"

//==============================================================================
Track::Track(MixerAudioSource &tracksMixer, bool stereo, int outputChannels)
	: audioThumbnailCache(1)
	, stereo(stereo)
	, tracksMixer(tracksMixer)
	, thread ("track")
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
	idLabel->setText("1 St", sendNotification);

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
	addAndMakeVisible(muteButton);

	//formatManager.registerBasicFormats();

	audioThumbnail = new AudioThumbnail(1000, formatManager, audioThumbnailCache);
}

Track::~Track()
{
	tracksMixer.removeInputSource(remappingAudioSource);
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
			"*.mp3");
	if (myChooser.browseForFileToOpen())
	{
		audioFile = File(myChooser.getResult());
		
		AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

		audioThumbnail->setSource(new FileInputSource(audioFile));
		repaint();

		currentAudioFileSource = new AudioFormatReaderSource (reader, true);
		// ..and plug it into our transport source
		transportSource.setSource (currentAudioFileSource,
								   32768, // tells it to buffer this many samples ahead
								   &thread, // this is the background thread to use for reading-ahead
								   reader->sampleRate);
	}
}

void Track::paint (Graphics& g)
{
	audioThumbnail->drawChannels(g, Rectangle<int>(100 + 40 + 40,0,getWidth(), getHeight()), 0, audioThumbnail->getTotalLength(), 1.0f);
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
	for (int channel = 0; channel < mapping.size(); ++channel) {
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