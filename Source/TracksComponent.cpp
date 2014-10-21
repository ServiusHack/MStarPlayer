/*
  ==============================================================================

    TracksComponent.cpp
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TracksComponent.h"

//==============================================================================
TracksComponent::TracksComponent(MixerComponent* mixer)
	: mixer(mixer)
{
	mixer->getMixerAudioSource().addInputSource(&tracksMixer, false);
	setBounds(0,0,100,100);
}

TracksComponent::~TracksComponent()
{
	mixer->getMixerAudioSource().removeInputSource(&tracksMixer);
}

void TracksComponent::paint (Graphics& g)
{
}

void TracksComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	int y = 0;
	int height = 100;
	for (int i = 0; i < tracks.size(); i++) {
		tracks.getUnchecked(i)->setBounds(0,y,getWidth(),height);
		y += height;
	}
	setBounds(getX(), getY(), getWidth(), y + height);
}

void TracksComponent::addMonoTrack()
{
	Track* track = new Track(tracksMixer, false);
	addAndMakeVisible(track);
	tracks.add(track);
}

void TracksComponent::addStereoTrack()
{
	Track* track = new Track(tracksMixer, true);
	addAndMakeVisible(track);
	tracks.add(track);
}

void TracksComponent::play()
{
	for (int i = 0; i < tracks.size(); ++i) {
		tracks[i]->play();
	}
}

void TracksComponent::pause()
{

}

void TracksComponent::stop()
{

}