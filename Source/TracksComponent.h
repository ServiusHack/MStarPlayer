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
	TracksComponent(MixerComponent* mixer, int outputChannels, PositionCallback positionCallback);
    ~TracksComponent();

    void paint (Graphics&);
    void resized();

	void addMonoTrack();
	void addStereoTrack();
	void addTrackFromXml(const XmlElement& element);

	void play();
	void pause();
	void stop();

	int playerCount();
	Track& player(int index);

	void setOutputChannels(int outputChannels);


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TracksComponent)

	OwnedArray<Track> tracks;

	MixerComponent* mixer;
	MixerAudioSource tracksMixer;

	PositionCallback positionCallback;

	int _outputChannels;
};


#endif  // TRACKSCOMPONENT_H_INCLUDED
