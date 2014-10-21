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
#include "MixerComponent.h"

//==============================================================================
/*
*/
class TracksComponent    : public Component
{
public:
    TracksComponent(MixerComponent* mixer);
    ~TracksComponent();

    void paint (Graphics&);
    void resized();

	void addMonoTrack();
	void addStereoTrack();

	void play();
	void pause();
	void stop();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TracksComponent)

	OwnedArray<Track> tracks;

	MixerComponent* mixer;
	MixerAudioSource tracksMixer;
};


#endif  // TRACKSCOMPONENT_H_INCLUDED
