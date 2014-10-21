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
Track::Track(MixerAudioSource &tracksMixer, bool stereo)
	: audioThumbnailCache(1)
	, stereo(stereo)
	, tracksMixer(tracksMixer)
	, thread ("track")
{
	formatManager.registerBasicFormats();
	thread.startThread (3);
	tracksMixer.addInputSource(&transportSource, false);

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
	tracksMixer.removeInputSource(&transportSource);
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

		//audioThumbnail->setReader(reader, 0);

		AudioFormatReaderSource* currentAudioFileSource = new AudioFormatReaderSource (reader, true);
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
	transportSource.start();
}