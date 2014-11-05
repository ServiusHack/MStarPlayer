#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"
#include "MixerComponent.h"

/*
*/
class TracksComponent : public Component
{
public:
	TracksComponent(MixerComponent* mixer, int outputChannels, PositionCallback positionCallback);
    ~TracksComponent();

    void resized();

	void addMonoTrack();
	void addStereoTrack();
	void addTrackFromXml(const XmlElement* element);
	void addTrack(bool stereo, const XmlElement* element = nullptr);

	void play();
	void pause();
	void stop();

	int playerCount();
	Track& player(int index);

	void setOutputChannels(int outputChannels);

private:
	OwnedArray<Track> m_tracks;

	MixerComponent* m_mixer;
	MixerAudioSource m_tracksMixer;

	PositionCallback m_positionCallback;

	int m_outputChannels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
