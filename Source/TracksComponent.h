/*
  ==============================================================================

    TracksComponent.h
    Created: 21 Jan 2014 1:15:08am
    Author:  User

  ==============================================================================
*/

#ifndef TRACKSCOMPONENT_H_INCLUDED
#define TRACKSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"

//==============================================================================
/*
*/
class TracksComponent    : public Component
{
public:
    TracksComponent();
    ~TracksComponent();

    void paint (Graphics&);
    void resized();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TracksComponent)

	OwnedArray<Track> tracks;
};


#endif  // TRACKSCOMPONENT_H_INCLUDED
