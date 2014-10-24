/*
  ==============================================================================

    PlaylistPlayerWindow.cpp
    Created: 29 Oct 2013 8:43:19pm
    Author:  User

  ==============================================================================
*/

#include <vector>
#include <algorithm>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaylistPlayerWindow.h"
#include "ChannelMapping.h"

//==============================================================================
PlaylistPlayerWindow::PlaylistPlayerWindow(MixerComponent* mixer_, int outputChannels_) :
      mixer (mixer_),
      outputChannels(outputChannels_),
      thread ("audio file preview")
{
	
	// play button
	playButton = new ImageButton("Play");
	Image normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstart_png, BinaryData::mediaplaybackstart_pngSize);
	playButton->addListener(this);
	playButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(playButton);
	
	// pause button
	pauseButton = new ImageButton("Pause");
	pauseButton->addListener(this);
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackpause_png, BinaryData::mediaplaybackpause_pngSize);
	pauseButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(pauseButton);
	
	// stop button
	stopButton = new ImageButton("Stop");
	stopButton->addListener(this);
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaplaybackstop_png, BinaryData::mediaplaybackstop_pngSize);
	stopButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(stopButton);
	
	// seek backward button
	seekBackwardButton = new ImageButton("Seek Backward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaseekbackward_png, BinaryData::mediaseekbackward_pngSize);
	seekBackwardButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(seekBackwardButton);
	
	// seek forward button
	seekForwardButton = new ImageButton("Seek Forward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaseekforward_png, BinaryData::mediaseekforward_pngSize);
	seekForwardButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(seekForwardButton);
	
	// skip backward button
	skipBackwardButton = new ImageButton("Skip Backward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipbackward_png, BinaryData::mediaskipbackward_pngSize);
	skipBackwardButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(skipBackwardButton);
	
	// skip forward button
	skipForwardButton = new ImageButton("Skip Forward");
	normalImage = ImageFileFormat::loadFrom (BinaryData::mediaskipforward_png, BinaryData::mediaskipforward_pngSize);
	skipForwardButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.0f);
	addAndMakeVisible(skipForwardButton);

	// configuration button
	configureButton = new ImageButton("Configure");
	normalImage = ImageFileFormat::loadFrom (BinaryData::emblemsystem_png, BinaryData::emblemsystem_pngSize);
	configureButton->setImages(true, true, true, 
                                normalImage, 0.7f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::transparentBlack,
                                normalImage, 1.0f, Colours::pink.withAlpha (0.8f),
                                0.5f);
	addAndMakeVisible(configureButton);
	configureButton->addMouseListener(this, false);

	addAndMakeVisible (digitalDisplay = new Label (String::empty, "00:00:00"));
    digitalDisplay->setFont (Font (Font::getDefaultMonospacedFontName (), 14, Font::plain));
    digitalDisplay->setColour (Label::textColourId, Colours::red);
    digitalDisplay->setColour (Label::backgroundColourId, Colours::white);

	addAndMakeVisible( tracksViewport = new Viewport());
	tracksViewport->setViewedComponent(tracks = new TracksComponent(mixer, outputChannels), false);

	
	mixer->registerPlayer(this);

	setSize(150, 80);
}

PlaylistPlayerWindow::~PlaylistPlayerWindow()
{
	mixer->unregisterPlayer(this);
}

void PlaylistPlayerWindow::setGain(float gain)
{

}

void PlaylistPlayerWindow::paint (Graphics& g)
{

}

void PlaylistPlayerWindow::resized()
{
	int buttonWidth = std::min(getWidth() / 11, 32);
	int buttonHeight = buttonWidth;
	playButton->setBounds(0 * buttonWidth, 0, buttonWidth, buttonHeight);
	pauseButton->setBounds(1 * buttonWidth, 0, buttonWidth, buttonHeight);
	stopButton->setBounds(2 * buttonWidth, 0, buttonWidth, buttonHeight);
	seekBackwardButton->setBounds(3 * buttonWidth, 0, buttonWidth, buttonHeight);
	seekForwardButton->setBounds(4 * buttonWidth, 0, buttonWidth, buttonHeight);
	skipBackwardButton->setBounds(5 * buttonWidth, 0, buttonWidth, buttonHeight);
	skipForwardButton->setBounds(6 * buttonWidth, 0, buttonWidth, buttonHeight);
	configureButton->setBounds(7 * buttonWidth, 0, buttonWidth, buttonHeight);
	digitalDisplay->setBounds(8 * buttonWidth, 0, 3 * buttonWidth, buttonHeight);
	tracksViewport->setBounds(0,buttonHeight, getWidth(), getHeight() - buttonHeight);
	tracks->setBounds(0,0, tracksViewport->getMaximumVisibleWidth(), tracks->getHeight());
}

void PlaylistPlayerWindow::setOutputChannels(int outputChannels_)
{
	outputChannels = outputChannels_;
	tracks->setOutputChannels(outputChannels);
}

void PlaylistPlayerWindow::mouseDown (const MouseEvent & event)
{
	if (event.eventComponent != configureButton)
		return;

	PopupMenu m;
	m.addItem (1, "add stereo track");
	m.addItem (2, "add mono track");
	m.addItem (3, "configure channels");
	const int result = m.show();

	if (result == 1) {
		tracks->addStereoTrack();
		repaint();
		parentSizeChanged();
	} else if (result == 2) {
		tracks->addMonoTrack();
		repaint();
	} else if (result == 3) {
		configureChannels();
	}
}

void PlaylistPlayerWindow::buttonClicked(Button * button)
{
	if (button == playButton) {
		tracks->play();
	} else if (button == pauseButton) {
		tracks->pause();
	} else if (button == stopButton) {
		tracks->stop();
	}
}

void PlaylistPlayerWindow::timerCallback()
{
}

XmlElement* PlaylistPlayerWindow::saveToXml() const
{
    XmlElement* element = new XmlElement("Player");
    element->setAttribute("type", "playlist");

	Rectangle<int> bounds = getBounds();

	XmlElement* boundsXml = new XmlElement("Bounds");
	boundsXml->setAttribute("x", bounds.getX());
	boundsXml->setAttribute("y", bounds.getY());
	boundsXml->setAttribute("width", bounds.getWidth());
	boundsXml->setAttribute("height", bounds.getHeight());
	element->addChildElement(boundsXml);

	XmlElement* nameXml = new XmlElement("Name");
	nameXml->addTextElement(getName());
	element->addChildElement(nameXml);

	return element;
}

void PlaylistPlayerWindow::restoreFromXml (const XmlElement& element)
{
	XmlElement* boundsXml = element.getChildByName("Bounds");

	String x = boundsXml->getStringAttribute("x", "0");
	String y = boundsXml->getStringAttribute("y", "0");
	String width = boundsXml->getStringAttribute("width", "150");
	String height = boundsXml->getStringAttribute("height", "150");
	setBounds(x.getIntValue(), y.getIntValue(), width.getIntValue(), height.getIntValue());

	XmlElement* nameXml = element.getChildByName("Name");
	setName(nameXml->getAllSubText().trim());
}

void PlaylistPlayerWindow::configureChannels()
{
	std::vector<int> mapping;
	for (int i = 0; i < tracks->playerCount(); ++i) {
		std::vector<int> playerMapping = tracks->player(i).getMapping();
		mapping.insert(mapping.end(), playerMapping.begin(), playerMapping.end());
	}
	/*for (int channel = 0; channel < outputChannels; ++channel) {
		mapping[chanenl] = (remappingAudioSource->getRemappedOutputChannel(channel));
	}*/

	new ChannelMappingWindow(outputChannels, mapping, [&](int source, int target) {

		for (int i = 0; i < tracks->playerCount(); ++i) {
			if (source - tracks->player(i).getNumChannels() <= 0) {
				tracks->player(i).setOutputChannelMapping(source, target);
				break;
			}
			source -= tracks->player(i).getNumChannels();
		}
	}, [&]() {
	});
}