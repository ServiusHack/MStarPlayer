/*
  ==============================================================================

	JinglePlayerWindow.cpp
	Created: 9 Sep 2013 4:37:45pm
	Author:  Severin Leonhardt

  ==============================================================================
*/

#include <list>
#include <stdint.h>

#include "../JuceLibraryCode/JuceHeader.h"

#include "JinglePlayerWindow.h"

//==============================================================================
JinglePlayerWindow::JinglePlayerWindow(MixerComponent* mixer_, int outputChannels_)
	: mixer (mixer_)
	, outputChannels(outputChannels_)
	, thread ("audio file preview")
	, playImage(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize))
	, stopImage(Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize))
{
	// progress bar
	progress = 0.0;
	progressBar = new ProgressBar(progress);
	progressBar->setPercentageDisplay(false);
	progressBar->setTextToDisplay("00:00:000");
	addAndMakeVisible(progressBar);

	// total duration text
	totalDurationText = new DrawableText();
	addAndMakeVisible(totalDurationText);
	totalDurationText->setJustification(Justification(Justification::centredRight));
	totalDurationText->setText("00:00:000");

	// play button
	playButton = new DrawableButton("Play", DrawableButton::ImageFitted);
	addAndMakeVisible(playButton);
	playButton->setImages(playImage);
	playButton->setEnabled(false);
	playButton->addListener(this);
	playButton->setEdgeIndent(30);

	// configuration button
	configureButton = new ImageButton("Configure");
	Image normalImage = ImageFileFormat::loadFrom (BinaryData::emblemsystem_png, BinaryData::emblemsystem_pngSize);
	configureButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.5f);
	addAndMakeVisible(configureButton);
	configureButton->addMouseListener(this, false);
	
	// audio setup
	formatManager.registerBasicFormats();
	thread.startThread (3);
	remappingAudioSource = new ChannelRemappingAudioSource(&transportSource, false);
	remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
	// TODO: Probably do this for outputChannels times.
	remappingAudioSource->setOutputChannelMapping(0, 0);
	remappingAudioSource->setOutputChannelMapping(1, 1);
	mixer->getMixerAudioSource().addInputSource(remappingAudioSource, false);
	mixer->registerPlayer(this);

	setBounds(0, 0, 300, 150);
}

JinglePlayerWindow::~JinglePlayerWindow()
{
	mixer->getMixerAudioSource().removeInputSource(remappingAudioSource);
	mixer->unregisterPlayer(this);
	transportSource.setSource (nullptr);
}


void JinglePlayerWindow::setGain(float gain)
{
	transportSource.setGain(gain);
}

void JinglePlayerWindow::setOutputChannels(int outputChannels_)
{
	outputChannels = outputChannels_;
	remappingAudioSource->setNumberOfChannelsToProduce(outputChannels);
}

void JinglePlayerWindow::paint (Graphics& g)
{
	int progressBarHeight = 20;
	int totalDurationTextWidth = 70;

	configureButton->setBounds(0, getHeight() - progressBarHeight, progressBarHeight, progressBarHeight);

	progressBar->setBounds(configureButton->getWidth(), getHeight() - progressBarHeight, getWidth() - configureButton->getWidth() - totalDurationTextWidth, progressBarHeight);

	totalDurationText->setBounds(getWidth() - totalDurationTextWidth, getHeight() - progressBarHeight, totalDurationTextWidth, progressBarHeight);

	playButton->setBounds(0,0, getWidth(), getHeight() - progressBarHeight);
}

void JinglePlayerWindow::resized()
{
}

void JinglePlayerWindow::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent != configureButton)
		return;

	PopupMenu m;
	m.addItem (1, "load file");
	m.addItem (2, "configure channels", currentAudioFileSource != nullptr);
	const int result = m.show();

	if (result == 1)
		loadFile();
	else if (result == 2)
		configureChannels();
}

void JinglePlayerWindow::loadFile()
{
	FileChooser myChooser ("Please select the audio file you want to load ...",
			File::getSpecialLocation (File::userHomeDirectory),
			formatManager.getWildcardForAllFormats());
	if (myChooser.browseForFileToOpen())
	{
		audioFile = File(myChooser.getResult());
		loadFileIntoTransport();
	}
}

void JinglePlayerWindow::configureChannels()
{
	if (channelMappingWindow.get() == nullptr) {
		std::vector<int> mapping(currentAudioFileSource->getAudioFormatReader()->numChannels, -1);
		for (size_t channel = 0; channel < mapping.size(); ++channel) {
			mapping[channel] = (remappingAudioSource->getRemappedOutputChannel(channel));
		}

		channelMappingWindow.set(new ChannelMappingWindow(outputChannels, mapping, [&](int source, int target) {
			remappingAudioSource->setOutputChannelMapping(source, target);
		}, [&]() {
			// clear is not working
			delete channelMappingWindow.release();
		}), true);
	}
	channelMappingWindow->addToDesktop();
	channelMappingWindow->toFront(true);

}

void JinglePlayerWindow::buttonClicked(Button * /*button*/)
{
	if (transportSource.isPlaying()) {
		playButton->setImages(playImage);
		transportSource.stop();
		transportSource.setPosition (0);
	}
	else {
		playButton->setImages(stopImage);
		transportSource.setPosition (0);
		transportSource.start();
	}
}

void JinglePlayerWindow::loadFileIntoTransport ()
{
	stopTimer();

	// unload the previous file source and delete it..
	transportSource.stop();
	transportSource.setSource (nullptr);
	currentAudioFileSource = nullptr;
	playButton->setImages(playImage);

	AudioFormatReader* reader = formatManager.createReaderFor (audioFile);

	playButton->setEnabled(reader != nullptr);

	if (reader != nullptr)
	{
		currentAudioFileSource = new AudioFormatReaderSource (reader, true);

		// ..and plug it into our transport source
		transportSource.setSource (currentAudioFileSource,
								   32768, // tells it to buffer this many samples ahead
								   &thread, // this is the background thread to use for reading-ahead
								   reader->sampleRate);
		startTimer(50);

		if (channelMappingWindow.get() != nullptr) {
			std::vector<int> mapping(currentAudioFileSource->getAudioFormatReader()->numChannels, -1);
			for (size_t channel = 0; channel < mapping.size(); ++channel) {
				mapping[channel] = (remappingAudioSource->getRemappedOutputChannel(channel));
			}

			channelMappingWindow->setMapping(mapping);

		}
	}
}

void JinglePlayerWindow::timerCallback()
{
	double position    = transportSource.getCurrentPosition();
	double totalLength = transportSource.getLengthInSeconds();
	
	if (position > totalLength) {
		playButton->setImages(playImage);
		position = 0;
		progress = 0.0;
	} else {
		progress = position / totalLength;
	}
	
	progressBar->setTextToDisplay(formatSeconds(position));
	totalDurationText->setText(formatSeconds(totalLength));
}


String JinglePlayerWindow::formatSeconds(double totalSeconds)
{
	int minutes      = ((int) totalSeconds / 60) % (60);
	int seconds      = ((int) totalSeconds) % 60;
	int milliseconds = ((int) (totalSeconds * 1000)) % 1000;

	return String::formatted("%02i:%02i:%03i", minutes, seconds, milliseconds);
}

XmlElement* JinglePlayerWindow::saveToXml() const
{
    XmlElement* element = new XmlElement("Player");
    element->setAttribute("type", "jingle");

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
	
	if (audioFile != File::nonexistent)
	{
		XmlElement* fileXml = new XmlElement("File");
		fileXml->addTextElement(audioFile.getFullPathName());
		element->addChildElement(fileXml);
	}

	XmlElement* channelMappingXml = new XmlElement("ChannelMapping");
	channelMappingXml->addChildElement(remappingAudioSource->createXml());
	element->addChildElement(channelMappingXml);

	return element;
}

void JinglePlayerWindow::restoreFromXml (const XmlElement& element)
{
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
		audioFile = File(fileXml->getAllSubText().trim());
		loadFileIntoTransport();
	}

	XmlElement* channelMappingXml = element.getChildByName("ChannelMapping");
	remappingAudioSource->restoreFromXml(*channelMappingXml->getChildElement(0));
}