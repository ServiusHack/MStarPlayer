#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"
#include "MixerComponent.h"

/*
*/
class TracksComponent : public Component
{
public:
	TracksComponent(MixerComponent* mixer, int outputChannels, Track::PositionCallback positionCallback, Player::ChannelCountChangedCallback channelCountChanged);
    ~TracksComponent();

	virtual void resized() override;

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

	void setGain(float gain);
	float getGain();

	void setMute(bool mute);
	bool getMute();

private:
	OwnedArray<Track> m_tracks;
	float m_gain;
	bool m_mute;

	MixerComponent* m_mixer;
	MixerAudioSource m_tracksMixer;

	Track::PositionCallback m_positionCallback;

	Player::ChannelCountChangedCallback m_channelCountChanged;

	int m_outputChannels;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TracksComponent)
};
