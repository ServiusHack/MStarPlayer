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
TracksComponent::TracksComponent()
{
	Track *track = new Track();
	addAndMakeVisible(track);
	tracks.add(track);

	track = new Track(false);
	addAndMakeVisible(track);
	tracks.add(track);

	setBounds(0,0,100,100);
}

TracksComponent::~TracksComponent()
{
}

void TracksComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
	
    g.fillAll (Colours::green);   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::lightblue);
    g.setFont (14.0f);
    g.drawText ("TracksComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void TracksComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
	int y = 0;
	int height = 50;
	for (int i = 0; i < tracks.size(); i++) {
		tracks.getUnchecked(i)->setBounds(0,y += 50,getWidth(),50);
	}
}
